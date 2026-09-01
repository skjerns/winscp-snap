<h1 align="center">
  <img src="snap/local/src/winscp.png" width="128" alt="WinSCP" />
  <br />
  winscp
</h1>

<p align="center"><b>This is the snap for WinSCP</b>, <i>"a free SFTP, FTP, WebDAV, S3 and SCP client"</i>, packaged with WINE. It works on Ubuntu, Fedora, Debian, and other major Linux distributions.</p>

## Install

    sudo snap install winscp
    sudo snap connect winscp:removable-media
    sudo snap connect winscp:cups-control
    sudo snap connect winscp:ssh-keys

([Don't have snapd installed?](https://snapcraft.io/docs/core/install))

`ssh-keys` is needed to reach `~/.ssh`: the `home` interface does not grant
access to dot-directories. Connect it only if you keep your keys there.

Until the [auto-connection request](#store-side-setup) is granted, the WINE
content interfaces must also be connected by hand:

    sudo snap connect winscp:wine-base-devel  wine-platform:wine-base-devel
    sudo snap connect winscp:wine-runtime-c24 wine-platform-runtime-core24:wine-runtime-c24

## How it works

The snap ships the official **WinSCP Portable** build and runs it on the WINE
provided by the `wine-platform` content snaps. The launcher is
[sommelier-core](https://github.com/mmtrt/sommelier-core), the same launcher
used by the `notepad-plus-plus` and `irfanview` snaps.

Your state lives in `$SNAP_USER_COMMON` (`~/snap/winscp/common/`) and is **not**
touched by updates:

| Path | Contents |
|---|---|
| `~/snap/winscp/common/.wine` | the wineprefix |
| `~/snap/winscp/common/winscp.ini` | WinSCP configuration and saved sessions |

`~/snap/winscp/current/winscp/` is a symlink farm onto the read-only payload in
`$SNAP`; sommelier rebuilds it whenever the snap version or revision changes.

WinSCP's own updater is switched off on first run
(`Interface\Updates\Period=0`) because it cannot write into a read-only snap.
Updates arrive through snapd instead, which refreshes about four times a day.

## Updating

Two independent loops, both automatic:

- **package to user** — snapd auto-refreshes installed snaps. Nothing to do.
- **upstream to package** — `.github/workflows/release-check.yml` runs every 12
  hours, scrapes the stable Portable version from
  [winscp.net](https://winscp.net/eng/downloads.php) (release candidates are
  filtered out), and appends a line to `.build` only when the version changed.
  That commit triggers `.github/workflows/main.yml`, which builds the snap and
  runs `snapcraft upload --release=stable`.

To force a rebuild without waiting, run either workflow from the Actions tab.

## Maintainer setup

### Store side setup

1. Register the name:

       snapcraft register winscp

   Since March 2024 every new name registration gets a manual review; expect a
   turnaround of about two working days.

2. Export store credentials and put them in the repository secret
   `STORE_LOGIN`:

       snapcraft export-login --acls package_access,package_push,package_update,package_release -

3. Create a GitHub personal access token with `repo` scope and put it in the
   repository secret `PAT`. `release-check.yml` needs it so that its
   auto-commit can trigger `main.yml`; a commit made with the default
   `GITHUB_TOKEN` does not fire other workflows.

4. File an auto-connection request in the snapcraft forum `store-requests`
   category for the `wine-base-devel` and `wine-runtime-c24` content interfaces
   and for `removable-media`. Content interfaces auto-connect only between
   snaps of the same publisher, and `wine-platform` belongs to a third party,
   so without this every user has to run the `snap connect` lines above.

### Building and testing

`--destructive-mode` builds against the host and needs the host release to
match the base, so it only works on Ubuntu 24.04. That is what the CI runner
does. Anywhere else, build in LXD:

    sudo snap install --classic snapcraft lxd
    sudo lxd init --auto
    snapcraft

Every push also attaches the built snap to its workflow run, so you can take
one from CI instead:

    gh run download --name winscp-snap

Then run the checks, which cover launching, ini seeding and the
upstream-version-change path:

    sudo tools/runtime-test ./winscp_*.snap

### Refreshing the icon

`snap/local/src/winscp.png` was extracted once from the `WinSCP.exe` PE
resources: `RT_GROUP_ICON` 20156, the 256x256 entry. It has been stable across
releases. To regenerate it:

    wrestool -x -t 14 usr/share/winscp/WinSCP.exe -o ico/
    icotool -x -w 256 -o . ico/*.ico

## Licence

The packaging in this repository is MIT; see [LICENSE.md](LICENSE.md). WinSCP
itself is GPL-3.0, Copyright (c) Martin Prikryl. The snap ships WinSCP's
`license.txt` and a pointer to the matching source archive under
`/usr/share/doc/winscp/`.
