set(STONEYDSP_BIQUADS_CPACK_IGNORE_LIST_FILE "${CMAKE_CURRENT_LIST_FILE}")
set(STONEYDSP_BIQUADS_CPACK_IGNORE_LIST "")
list(APPEND STONEYDSP_BIQUADS_CPACK_IGNORE_LIST

    _CPack_Packages
    CMake
    JUCE
    pluginval
    pluginval_*

    /.cache
    /.config
    /.local

    /*.zip
    /*.tar
    /*.tar.*

    /.env*
    /.git/*
    /.bak

    /.certs
    /.cmake
    /.dotnet
    /.github
    /.gnome
    /.gnupg
    /.npm
    /.nvm
    /.pki
    /.ssh
    /.subversion
    /.vcpkg
    /.vs
    /.vscode
    /.vscode-remote-containers
    /.vscode-server
    /.w3m
    /.yarn

    /Desktop
    /Development
    /Documents
    /Downloads
    /Music
    /Pictures
    /Public
    /Templates
    /Videos

    /.bash_history
    /.bash_logout
    /.bash_profile
    /.bashrc
    /.i18n
    /.lesshst
    /.profile
    /.sudo_as_admin_successful
    /.wget-hsts
    /.yarnrc

    /doc
    /docs
    #/bin
    #/lib
    /usr

    /dist
    #/build
    #/Builds
    /install
    #/out

    /Builds

    /Release
    /Debug
    /MinSizeRel
    /RelWithDebInfo

    /downloads
    /packages
    /installed
    /vcpkg
    /vcpkg_installed

    /.*build.*

    /node_modules
    /package-lock.json
    /yarn.lock

    /\\\\.DS_Store
)
