set( NAME trackerAruco )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwRuntime
    fwCore
    fwData
    fwDataTools
    fwServices
    fwTools
    fwMath
    fwComEd
    fwCom
    arData
    extData
    fwMedData
    
    tracker
)
set( REQUIREMENTS dataReg
                  servicesReg
                  extDataReg
                  arDataReg
)
