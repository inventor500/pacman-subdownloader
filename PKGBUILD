pkgname='pacman-subdownloader'
pkgver='0.1'
pkgrel=1
pkgdesc="Download packages over a proxy while print the file name"
arch=('any')
url="https://github.com/inventor500/pacman-subdownloader"
license=('GPL3')
depends=('pacman' 'curl')
makedepends=('go')
source=('cmd/pacman-subdownloader/main.go')
sha256sums=('0f4c887b81a14c844f4f4449fa58b6ea37ccceeffeb3f2fbef0cd7953096c45c')

prepare() {
	mkdir "$pkgname"
	cd "$pkgname"
	mv ../main.go .
	go mod init "${url#https://}" # strip https:// from canonical URL
	go mod tidy
}

build() {
	export CGO_CPPFLAGS="${CPPFLAGS}"
	export CGO_CFLAGS="${CFLAGS}"
	export CGO_CXXFLAGS="${CXXFLAGS}"
	export CGO_LDFLAGS="${LDFLAGS}"
	export GOFLAGS="-buildmode=pie -trimpath -ldflags=-linkmode=external -mod=readonly -modcacherw"
	go build
}

package() {
	cd "$pkgname"
	install -Dm755 $pkgname "$pkgdir/usr/bin/$pkgname"
}
