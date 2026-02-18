#!/usr/bin/env bash
set -euo pipefail

usage() {
  echo "Usage: generate_package_channel_manifests.sh <version> <asset_url> <asset_sha256> <output_dir>" >&2
}

if [[ $# -ne 4 ]]; then
  usage
  exit 2
fi

version="$1"
asset_url="$2"
asset_sha256="$3"
output_dir="$4"

mkdir -p "${output_dir}/homebrew"
mkdir -p "${output_dir}/scoop"
mkdir -p "${output_dir}/winget"
mkdir -p "${output_dir}/chocolatey/tools"

cat > "${output_dir}/homebrew/manim-cpp.rb" <<EOF
class ManimCpp < Formula
  desc "C++ animation engine for explanatory math videos"
  homepage "https://github.com/ManimCommunity/manim"
  version "${version}"
  url "${asset_url}"
  sha256 "${asset_sha256}"
  license "MIT"

  def install
    bin.install "bin/manim-cpp"
    bin.install "bin/manim"
    bin.install "bin/manimce"
    bin.install "bin/manim-cpp-migrate"
    bin.install "bin/manim-cpp-extract-frames"
    (etc/"manim-cpp").install "config/manim.cfg.default"
    (share/"manim-cpp/shaders").install Dir["share/shaders/*"]
  end
end
EOF

cat > "${output_dir}/scoop/manim-cpp.json" <<EOF
{
  "version": "${version}",
  "description": "C++ animation engine for explanatory math videos",
  "homepage": "https://github.com/ManimCommunity/manim",
  "license": "MIT",
  "architecture": {
    "64bit": {
      "url": "${asset_url}",
      "hash": "${asset_sha256}"
    }
  },
  "extract_dir": "",
  "bin": [
    "bin/manim-cpp",
    "bin/manim",
    "bin/manimce",
    "bin/manim-cpp-migrate",
    "bin/manim-cpp-extract-frames"
  ]
}
EOF

cat > "${output_dir}/winget/manim-cpp.yaml" <<EOF
PackageIdentifier: ManimCommunity.ManimCpp
PackageVersion: ${version}
PackageName: manim-cpp
Publisher: Manim Community
License: MIT
ShortDescription: C++ animation engine for explanatory math videos
Installers:
  - Architecture: x64
    InstallerType: portable
    InstallerUrl: ${asset_url}
    InstallerSha256: ${asset_sha256}
    Commands:
      - manim-cpp
      - manim
      - manimce
      - manim-cpp-migrate
      - manim-cpp-extract-frames
ManifestType: singleton
ManifestVersion: 1.9.0
EOF

cat > "${output_dir}/chocolatey/manim-cpp.nuspec" <<EOF
<?xml version="1.0"?>
<package>
  <metadata>
    <id>manim-cpp</id>
    <version>${version}</version>
    <authors>Manim Community</authors>
    <projectUrl>https://github.com/ManimCommunity/manim</projectUrl>
    <description>C++ animation engine for explanatory math videos</description>
    <licenseUrl>https://github.com/ManimCommunity/manim/blob/main/LICENSE</licenseUrl>
    <requireLicenseAcceptance>false</requireLicenseAcceptance>
    <tags>manim cpp animation</tags>
  </metadata>
</package>
EOF

cat > "${output_dir}/chocolatey/tools/chocolateyinstall.ps1" <<EOF
\$ErrorActionPreference = 'Stop'
\$packageName = 'manim-cpp'
\$toolsDir = Split-Path -Parent \$MyInvocation.MyCommand.Definition
\$url64 = '${asset_url}'
\$checksum64 = '${asset_sha256}'

Install-ChocolateyZipPackage -PackageName \$packageName -Url64bit \$url64 -UnzipLocation \$toolsDir -Checksum64 \$checksum64 -ChecksumType64 'sha256'
EOF

echo "Generated package channel manifests in ${output_dir}"
