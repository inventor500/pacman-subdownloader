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
            'dea41646cdb1dda8dd6d2b458eb75d991bbdb5ce5bf0bbc06a7f76b4f2e55924')

prepare() {
	cmake -B build -S .
}

build() {
    cmake --build build
}

package() {
	install -Dm755 "build/$pkgname" "$pkgdir/usr/bin/$pkgname"
}
