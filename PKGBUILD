pkgname='pacman-subdownloader'
pkgver='0.2'
pkgrel=1
pkgdesc="Download packages over a proxy while print the file name"
arch=('any')
url="https://github.com/inventor500/pacman-subdownloader"
license=('GPL3')
depends=('pacman' 'curl')
makedepends=('cmake')
source=('CMakeLists.txt' 'main.cpp')
sha256sums=('4a699f5e549d3207cbbfa21bea62a86fe1828351f1c24d08ad49ba1694fac3df'
            '706c4b1b84e36064d757d115d5ffecc841845207fbcc226f19a29a7d4ccdcbe2')

prepare() {
	cmake -B build -S .
}

build() {
    cmake --build build
}

package() {
	install -Dm755 "build/$pkgname" "$pkgdir/usr/bin/$pkgname"
}
