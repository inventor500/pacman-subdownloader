pkgname='pacman-subdownloader'
pkgver='0.3.5'
pkgrel=1
pkgdesc="Download packages over a proxy while print the file name"
arch=('any')
url="https://github.com/inventor500/pacman-subdownloader"
license=('GPL3')
depends=('pacman' 'curl')
makedepends=('cmake')
source=('CMakeLists.txt' 'main.hpp' 'main.cpp')
sha256sums=('929bada693b9349ce8d8f7b5abb23538244d9298b9b28c776fdcfa67a6424bad'
            '7883b28bca522656e03e8e265e1782fe8b41ca81e0ba717d315e641c47668534'
            '76f4115ad21f222e1147897ddae544494d54ac4fc241146dd494fb8aa25eba96')

prepare() {
	cmake -B build -S .
}

build() {
    cmake --build build
}

package() {
	install -Dm755 "build/$pkgname" "$pkgdir/usr/bin/$pkgname"
}
