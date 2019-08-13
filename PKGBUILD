pkgname=dwm-kn
pkgver=6.2
pkgrel=2
pkgdesc="My own fork of dwm"
url="https://dwm.suckless.org/"
arch=('x86_64')
license=('MIT')
license=('GPL')
depends=('libx11' 'libxinerama' 'libxft' 'freetype2' 'dmenu')
provides=(dwm)
conflicts=(dwm)

source=()
md5sums=()

builddir=$(pwd)

build() {
  cd $builddir
	make
}

package() {
  cd $builddir
	make DESTDIR="$pkgdir" PREFIX=/usr install
	install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
