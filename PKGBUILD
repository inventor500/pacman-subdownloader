pkgname='pacman-subdownloader'
pkgver='0.3.1'
pkgrel=1
pkgdesc="Download packages over a proxy while print the file name"
arch=('any')
url="https://github.com/inventor500/pacman-subdownloader"
license=('GPL3')
depends=('pacman' 'curl')
makedepends=('cmake')
source=('CMakeLists.txt' 'main.hpp' 'main.cpp')
sha256sums=('4a699f5e549d3207cbbfa21bea62a86fe1828351f1c24d08ad49ba1694fac3df'
            '706e703880211de90e4f1a6e84994c606ba5093cb86c3b4b06bc548baf43ab18'
            '5f01fc5b71f94a61dcfe7e8c9c5a903215a80a614de83f347b95cdea736162ed')

prepare() {
	cmake -B build -S .
}

build() {
    cmake --build build
}

package() {
	install -Dm755 "build/$pkgname" "$pkgdir/usr/bin/$pkgname"
}
