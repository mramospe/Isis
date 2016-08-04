#/////////////////////////////////////////////////////////////
#//                                                         //
#//  Python modules                                         //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  AUTHOR: Miguel Ramos Pernas                            //
#//  e-mail: miguel.ramos.pernas@cern.ch                    //
#//                                                         //
#//  Last update: 04/08/2016                                //
#//                                                         //
#// ------------------------------------------------------- //
#//                                                         //
#//  Description:                                           //
#//                                                         //
#//  Definition of some tools to perform analysis using     //
#//  the package RooFit.                                    //
#//                                                         //
#// ------------------------------------------------------- //
#/////////////////////////////////////////////////////////////


from ROOT import gRandom, RooArgList, RooArgSet, RooConstVar, RooFormulaVar, RooRealVar, TRandom3, Double
from ROOT.RooFit import Binning, Name, NormRange, Range, Save
from Isis.PlotTools import MakeHistogram


#_______________________________________________________________________________
# This class allows to perform blind analysis. A random quantity is added to the
# existing variable, allowing it even to take values with the opposite sign. In
# order to do so, the range of the input variable is changed according to a
# given scale.
class BlindVar:
    
    def __init__( self, roovar, **kwargs ):
        '''
        To build the class, the variable to be blinded must be provided. In < scale >
        one modifies the range of values for the blinded variable. Also the names for
        the constant variable and the generated formula can be modified.
        '''
        
        name = roovar.GetName()
        print 'Defining a new blinded variable for <', name, '>'

        scale  = kwargs.get( 'scale', 1000 )
        eqname = kwargs.get( 'bvarname', name + '_BlindExpr' )

        ''' The seed is different for each call, which guarantees the blinding procedure '''
        print 'Generating new set of random bounds'
        rndm = TRandom3( 0 )
        vmin = roovar.getMin()
        boundlo = rndm.Uniform( vmin, vmin*scale )
        vmax = roovar.getMax()
        boundhi = rndm.Uniform( vmax, vmax*scale )

        clonename = name + '_BLIND'
        blindVar = RooRealVar( clonename, clonename, roovar.getVal(), boundlo, boundhi )
        print 'Created clone variable named <', clonename, '>'
        
        alpha = ( vmax - vmin )/( boundhi - boundlo )
        beta  = ( vmax*boundlo - vmin*boundhi )/( vmax - vmin )

        blindVar.setVal( alpha*( roovar.getVal() - beta ) )

        alpha = RooConstVar( 'alpha', 'alpha', alpha )
        beta  = RooConstVar( 'beta', 'beta', beta )
        
        formula = 'alpha*( %s - beta)' % clonename
        varlist = RooArgList( blindVar, alpha, beta )
        blindEq = RooFormulaVar( eqname, eqname, formula, varlist )
        print 'The blinding formula is:', formula
        
        self.Alpha    = alpha
        self.Beta     = beta
        self.BlindEq  = blindEq
        self.BlindVar = blindVar

    def GetTrueValue( self ):
        '''
        Returns the true value of the blinded variable
        '''
        alpha   = self.Alpha.getVal()
        truevar = self.BlindVar
        result  = alpha*( truevar.getVal() - self.Beta.getVal() )
        error   = alpha*truevar.getError()
        errorlo = alpha*truevar.getErrorLo()
        errorhi = alpha*truevar.getErrorHi()
        return result, error, errorlo, errorhi
    
    def GetUnblindVar( self ):
        '''
        Returns the RooFormulaVar object that must be used to do the fit
        '''
        return self.BlindEq

    def PrintTrueValue( self ):
        '''
        Displays the true value of the blinded variable
        '''
        name   = self.BlindVar.GetName()
        outstr = ( name, ) + self.GetTrueValue()
        print 'Unblinded result: %s = %.4g +/- %.4g (%.4g) (+%.4g)' % outstr

#_______________________________________________________________________________
# This function extracts the values of a pull to a list, useful to check if
# the result matches a gaussian centered at zero and sigma equal to one
def ExtractPullValues( pullplot ):
    xbin = Double( 0. )
    ybin = Double( 0. )
    np   = pullplot.GetN()
    lst  = np*[ 0. ]
    for ib in xrange( pullplot.GetN() ):
        pullplot.GetPoint( ib, xbin, ybin )
        lst[ ib ] += ybin
    return lst

#_______________________________________________________________________________
# Class to book the results for a set of variables in a fit. A link to the Root
# variables is created, so it can update their value using the method < Update >.
class FitContainer( dict ):

    def __init__( self, *args ):
        '''
        As input, a list with the different RooAbsReal objects must be provided. The
        variables, values and errors will be booked.
        '''
        for var in args:
            name = var.GetName()
            if '_' in name:
                print 'ERROR: No whitespaces are allowed in the names of the variables'
            if name in self:
                print 'ERROR: Variable with name <', name, '> already in class'

            dic = {
                'var'     : var,
                'value'   : var.getVal(),
                'error'   : var.getError(),
                'errorlo' : var.getErrorLo(),
                'errorhi' : var.getErrorHi()
                }

            self[ name ] = dic

    def ListOfVariables( self ):
        '''
        Returns a list with all the different variables booked within this class
        '''
        return [ self[ name ][ 'var' ] for name in self ]

    def Print( self ):
        '''
        Displays the information of the different variables in the class
        '''
        maxv = max( len(name) for name in self )
        if maxv < 4:
            maxv = 4
        maxv += 1

        out = '\n'
        for name, dic in self.iteritems():
            out  += (maxv - len(name))*' ' + name
            val   = dic['value']
            err   = dic['error']
            errlo = dic['errorlo']
            errhi = dic['errorhi']
            for el in (val, err, errlo, errhi):
                addstr  = '%.4g' % el
                out    += ( 13 - len(addstr) )*' ' + addstr
            out += '\n'

        frstl  = (maxv - 4)*' ' + 'Name'
        frstl += 8*' ' + 'Value'
        frstl += 8*' ' + 'Error'
        frstl += 6*' ' + 'ErrorLo'
        frstl += 6*' ' + 'ErrorHi'
        
        deco = (maxv + 53 )*'*'
        
        print deco + '\n' + frstl + '\n' + deco + out + deco
            
    def Update( self ):
        '''
        Updates the content for each of the variables booked within this class
        '''
        for name, data in self.iteritems():
            var = data['var']
            var.setVal( data['value'] )
            var.setError( data['error'] )

#_______________________________________________________________________________
# Class to perform stability studies of the fits
class FitPerformance:

    def __init__( self, mainpdf, indepVars, depVars, yields, **kwargs ):
        '''
        One must provide the pdf of the fit, the independent and dependent variables, and the
        yields. The fit options can be provided in **kwargs as < fitopts >.
        '''
        
        self.FitOpts = kwargs.get( 'fitopts', [] )
        opt = Save( True )
        if opt not in self.FitOpts:
            self.FitOpts.append( opt )
        
        self.DepVars   = FitContainer( *depVars )
        self.IndepVars = indepVars
        self.MainPdf   = mainpdf
        self.Yields    = FitContainer( *yields )

    def _stabilitymsg( self ):
        '''
        Message to be displayed by all the stability test methods
        '''
        print '*********************************'
        print 'Performing new stability analysis'
        print '*********************************'
        
    def StbTestByNumberOfEvents( self, npoints, vmin, step ):
        '''
        Performs a test of stability by changin the number of events in a sample generated
        by the current values of the variables in the pdf.
        '''

        self._stabilitymsg()

        print '-- Initializing stability test by number of events --'
        print '-- Number of points:      ', npoints
        print '-- Inicial number of evts:', vmin
        print '-- Events per step:       ', step

        mainPdf = self.MainPdf

        output = []

        for nevts in xrange( vmin, vmin + npoints*step, step ):
            
            out = {}

            deco = ( '************************************************' +
                     len(str(nevts))*'*' + '**' )
            print deco
            print 'Generating new data set with number of events <', nevts, '>'
            print deco
            
            dataSet = mainPdf.generate( RooArgSet( *self.IndepVars ), nevts )
            
            results = mainPdf.fitTo( dataSet, *self.FitOpts )

            out[ 'data' ]      = dataSet
            out[ 'nevts' ]     = nevts
            out[ 'results' ]   = results
            out[ 'variables' ] = FitContainer( *self.DepVars.ListOfVariables() )
            out[ 'yields' ]    = FitContainer( *self.Yields.ListOfVariables() )
            
            output.append( out )

        return output

    def StbTestByRandomYields( self, npoints, nevts, shape = 'Poisson' ):
        '''
        Performs an stability test by randomizing the yield variables in the main pdf, following
        a shape that can be poissonian or gaussian (considering the error of the yields)
        '''
        
        self._stabilitymsg()
        
        print '-- Initializing stability test by random yields --'
        print '-- Number of points:', npoints
        print '-- Number of events:', nevts
        print '-- Random shape:    ', shape

        rndm = TRandom3()

        output = []

        for ip in xrange( npoints ):

            out   = {}
            cvals = {}

            for name, dic in self.Yields.iteritems():
            
                var = dic['var']

                cval = dic['value']
                cerr = dic['error']
                vmin = var.getMin()
                vmax = var.getMax()

                cvals[ name ] = cval
                print cval, vmin, vmax
            
                val = 2*abs( vmax )
            
                while val < vmin or val > vmax:

                    if shape == 'Poisson':
                        val = rndm.PoissonD( cval )
                    elif shape == 'Gaussian':
                        val = rndm.Gaus( cval, cerr )
                    else:
                        print 'ERROR: Unknown shape <', shape, '>'
                        return
                
                var.setVal( val )

            print '*********************************'
            print ' Set new configuration of yields '
            FitContainer( *self.Yields.ListOfVariables() ).Print()
                
            dataSet = self.MainPdf.generate( RooArgSet( *self.IndepVars ), nevts )
            
            for name, dic in self.Yields.iteritems():
                dic['var'].setVal( cvals[ name ] )
            
            results = self.MainPdf.fitTo( dataSet, *self.FitOpts )
            
            out[ 'data' ]      = dataSet
            out[ 'nevts' ]     = nevts
            out[ 'results' ]   = results
            out[ 'variables' ] = FitContainer( *self.DepVars.ListOfVariables() )
            out[ 'yields' ]    = FitContainer( *self.Yields.ListOfVariables() )
            
            output.append( out )

        return output

#_______________________________________________________________________________
# Makes the pull plot for a given variable, data set, pdf and the number of bins
# for the plot. If the option < pull > is set to false, the result will be the
# residual plot.
def MakePullPlot( nbins, dataset, roovar, pdf, pull = True, **kwargs ):
    
    limits = kwargs.get( 'limits', [] )
    if limits:
        limdic = {}
        for el in limits:
            ibin = roovar.getBinningPtr( el )
            limdic[ el ] = { 'pdf': 0, 'range': ( ibin.lowBound(), ibin.highBound() ) }
    delpoints = []
    
    frame = roovar.frame()
    
    ''' This is the graph that afterwards will be returned '''
    dataset.plotOn( frame, Binning( nbins ), Name( 'hist' ) )
    graphDst = frame.getHist( 'hist' )

    '''
    If limits are specified, it plots all the different curves in all the different ranges. This
    must be done since RooFit is not capable to manage curves covering many ranges.
    '''
    if limits:
        for kw, el in limdic.iteritems():
            name = 'pdf_' + kw
            pdf.plotOn( frame, Name( name ), Range( kw ) )
            el[ 'pdf' ] = frame.getCurve( name )
    else:
        pdf.plotOn( frame, Name( 'pdf0' ) )
        graphPdf = frame.getCurve( 'pdf0' )
    
    xbin = Double( 0. )
    ybin = Double( 0. )
    
    ''' Makes a loop over all the points in the graph to correct them '''
    maxerr = 0
    accept = True
    for ib in xrange( nbins ):

        graphDst.GetPoint( ib, xbin, ybin )

        ''' Checks whether the value is inside any range specified in the input '''
        if limits:
            accept = False
            for kw, el in limdic.iteritems():
                graphPdf   = el[ 'pdf' ]
                vmin, vmax = el[ 'range' ]
                if vmin <= xbin and xbin < vmax:
                    accept = True
                    break
        
        ''' If the point is not accepted it is removed from the graph at the end of the process '''
        if accept:

            errhi = graphDst.GetErrorYhigh( ib )
            errlo = graphDst.GetErrorYlow( ib )

            dst = ybin - graphPdf.Eval( xbin )

            if dst > 0:
                error = errlo
            else:
                error = errhi
        
            if pull:
                dst /= error
                graphDst.SetPointEYhigh( ib, 1 )
                graphDst.SetPointEYlow( ib, 1 )
                factor = 5
            else:
                if error > maxerr:
                    maxerr = error
                factor = 5*error

            '''
            Displays a warning message if any point if more far than 5 times the error
            from the fit curve
            '''
            if abs( dst ) > factor:
                print 'WARNING: Point %i at < %.2f > exceeds 5 sigma from the fit curve' %(ib, xbin)
        
            graphDst.SetPoint( ib, xbin, dst )

        else:
            delpoints.append( ib )

    ''' Deletes the different points that are not in the specified ranges '''
    delpoints.reverse()
    for el in delpoints:
        graphDst.RemovePoint( el )

    if pull:
        graphDst.SetMinimum( -5. )
        graphDst.SetMaximum( +5. )
    else:
        graphDst.SetMinimum( -5*maxerr )
        graphDst.SetMaximum( +5*maxerr )

    graphDst.GetXaxis().SetLimits( roovar.getMin(), roovar.getMax() )

    return graphDst
