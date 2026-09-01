Title: Auto-connection request for the winscp snap (wine-platform content
interfaces and removable-media)

Category: store-requests

---

Hello,

I have published `winscp`, a snap of [WinSCP](https://winscp.net), a
GPL-3.0 SFTP/FTP/WebDAV/S3/SCP client. It runs the official WinSCP Portable
build on WINE, using the `wine-platform` content snaps and the sommelier-core
launcher, in the same way as the existing `notepad-plus-plus`, `irfanview` and
`acrordrdc` snaps.

Packaging: https://github.com/skjerns/winscp-snap

I would like to request auto-connection for the following:

**Content interfaces**

* plug `wine-base-devel` to slot `wine-platform:wine-base-devel`
* plug `wine-runtime-c24` to slot `wine-platform-runtime-core24:wine-runtime-c24`

These provide the WINE build and its runtime. They are published by another
developer (mmtrt), so they do not auto-connect on publisher identity, and
without them the snap cannot start at all: sommelier exits with a message
asking the user to run `snap connect` by hand. There is precedent in the
earlier auto-connection requests for the `wine-platform-*` snaps.

**removable-media**

WinSCP is a file transfer client, so transferring to and from a mounted USB
disk or a second drive is a core use of the application, not an incidental
one. Without this the file browser only sees `$HOME`.

I am not requesting auto-connection for `ssh-keys`. The snap plugs it so that
users who keep OpenSSH keys in `~/.ssh` can import them, but that should stay
a deliberate manual connection.

Thank you.
