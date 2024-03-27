#[=============================================================================[
    Simple two-pole equalizer with variable oversampling.
    Copyright (c) 2024 - Nathan J. Hood

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
.
]=============================================================================]#

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

    #/Release
    #/Debug
    #/MinSizeRel
    #/RelWithDebInfo

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
