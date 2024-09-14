set(ENKI_COMMON_PROPERTIES
  C_STANDARD 11
  C_STANDARD_REQUIRED TRUE
  C_EXTENSIONS OFF
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED TRUE
  CXX_EXTENSIONS OFF
)

if (${MSVC})
  set(ENKI_COMMON_COMPILE_OPTIONS
    $<$<COMPILE_LANGUAGE:CXX>:
    /W3
    #---moved warning toward level 4---
    /w44244 #conversion toward smaller int type, possible data loss
    /w44305 #conversion toward smaller type in initialization or ctor argument, possible data loss
    /w44267 #conversion of size_t toward smaller type, possible data loss
    #----------------------------------
    /WX
    >
  )
else()
  set(ENKI_COMMON_COMPILE_OPTIONS
    $<$<COMPILE_LANGUAGE:CXX>:
    -Wall
    -Wextra
    -Werror
    -pedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wuseless-cast
    -Wcast-align
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wsuggest-override
    -Wno-unknown-warning-option
    >
  )
endif()

