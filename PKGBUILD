pkgname='pacman-subdownloader'
pkgver='0.3.4'
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
            '1e1271a5284ee6dfa579a4f98d066a1ce668951873558eb0877c3a7fe1b533ec')

prepare() {
	cmake -B build -S .
}

build() {
    cmake --build build
}

package() {
	install -Dm755 "build/$pkgname" "$pkgdir/usr/bin/$pkgname"
}
