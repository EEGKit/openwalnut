# Include needed lists
if( OW_MODULE_TOOLBOX_BASE OR OW_MODULE_TOOLBOX_ALL )
  include( modules-base.toolbox )
endif( OW_MODULE_TOOLBOX_BASE OR OW_MODULE_TOOLBOX_ALL )

if( OW_MODULE_TOOLBOX_EEG OR OW_MODULE_TOOLBOX_ALL )
  include( modules-eeg.toolbox )
endif( OW_MODULE_TOOLBOX_EEG OR OW_MODULE_TOOLBOX_ALL )

if( OW_MODULE_TOOLBOX_IMAGEPROCESSING OR OW_MODULE_TOOLBOX_ALL )
  include( modules-imageprocessing.toolbox )
endif( OW_MODULE_TOOLBOX_IMAGEPROCESSING OR OW_MODULE_TOOLBOX_ALL )

  if( OW_MODULE_TOOLBOX_IO OR OW_MODULE_TOOLBOX_ALL )
  include( modules-io.toolbox )
endif( OW_MODULE_TOOLBOX_IO OR OW_MODULE_TOOLBOX_ALL )

if( OW_MODULE_TOOLBOX_OTHERS OR OW_MODULE_TOOLBOX_ALL )
  include( modules-others.toolbox )
endif( OW_MODULE_TOOLBOX_OTHERS OR OW_MODULE_TOOLBOX_ALL )

# optional user toolbox which is not unter version control allowing all to only compile their modules needed.
include( modules-user.toolbox OPTIONAL )

