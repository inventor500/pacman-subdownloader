pkgname='pacman-subdownloader'
pkgver='0.3'
pkgrel=1
pkgdesc="Download packages over a proxy while print the file name"
arch=('any')
url="https://github.com/inventor500/pacman-subdownloader"
license=('GPL3')
depends=('pacman' 'curl')
makedepends=('cmake')
source=('CMakeLists.txt' 'main.hpp' 'main.cpp' 'packages.hpp' 'packages.cpp')
sha256sums=('af86ce91473e438b4b4c1f276a16a7f3dadaccbe28469486b7c53fb531e62599'
            'dcc7ff569b22593e08a54b85bb3aed7b8168082600bbf5d0d16d30e440798778'
            'dc3c32eceaa207892843ca13448bbcd69b43a3cab1c1dc17ebb01eaf85aaa66c'
            '91233af87105223435f07652e3c0e63993319351d61901285169c73dac8e8f12'
            '3931a504978fa4512ac03819736839c8c5f533f4165d385e0e02a85e0e8f6b9c')

prepare() {
	cmake -B build -S .
}

build() {
    cmake --build build
}

package() {
	install -Dm755 "build/$pkgname" "$pkgdir/usr/bin/$pkgname"
}
