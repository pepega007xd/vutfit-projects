echo "starting..."

echo "task 1: creating file /var/tmp/ukol.img of size 200MB"
dd if=/dev/zero of=/var/tmp/ukol.img bs=200M count=1 && echo "successfully created file" || echo "file creation failed"
echo ""

echo "task 2: creating loop device ukol.img"
losetup --find /var/tmp/ukol.img && echo "loop device created" || echo "loop device creation failed"
echo ""

echo "task 3: create ext4 filesystem on newly created loop device"
# store new loopdevice path to $LOOP_DEVICE
LOOP_DEVICE="$(losetup --list --noheadings --output NAME | tail -n1)"
mkfs.ext4 "$LOOP_DEVICE" && echo "filesystem created" || echo "couldn't create filesystem"
echo ""

UKOL_PATH="/var/www/html/ukol"
echo "task 4: modify /etc/fstab to mount ukol.img to $UKOL_PATH"
echo "/var/tmp/ukol.img $UKOL_PATH ext4 loop 0 0" >> /etc/fstab \
&& echo "fstab modified" || echo "failed to modify fstab"
systemctl daemon-reload && echo "reloaded systemd daemons (fstab changed)"
echo ""

echo "task 5: mount ukol.img to its location"
echo "creating empty directory $UKOL_PATH to mount to"
mkdir -p $UKOL_PATH && echo "directory created" || echo "cannot create directory"
echo "mounting ukol.img"
mount /var/tmp/ukol.img && echo "ukol.img mounted" || echo "couldn't mount ukol.img"
echo ""

echo "task 6: download packages provided via arguments into $UKOL_PATH"
echo "downloading packages: $@"
yum download --destdir=$UKOL_PATH $@ \
&& echo "packages downloaded" || echo "couldn't download packages"
echo ""

echo "task 7: generate repodata in $UKOL_PATH"
echo "installing createrepo utility"
yum install -y createrepo && echo "createrepo installed" || echo "failed to install createrepo"
createrepo $UKOL_PATH && echo "repo metadata generated" || echo "couldn't create repo"
echo "fixing SELinux context in $UKOL_PATH"
restorecon -Rv $UKOL_PATH && echo "SELinux context fixed" || echo "failed to fix SELinux context"
echo ""

echo "task 8: configure /etc/yum.repos.d/ukol.repo to url repo http://localhost/ukol"
echo "[ukol]
name=ukol
baseurl=http://localhost/ukol
enabled=1" > /etc/yum.repos.d/ukol.repo \
&& echo "repo configued" || echo "couldn't configure repo"
echo ""

echo "task 9: install and run httpd (Apache) server"
# repo ukol is not available yet, disabling it
yum install --disablerepo ukol -y httpd && echo "installed httpd" || echo "could't install httpd"
systemctl enable --now httpd && echo "started httpd" || echo "couldn't start httpd"
echo ""

echo "task 10: print available yum repos"
yum repolist
yum repolist | grep "ukol" && echo "repo 'ukol' found in repolist" || echo "repo 'ukol' wasn't found"
echo ""

echo "task 11: unmount $UKOL_PATH"
umount -f $UKOL_PATH && echo "$UKOL_PATH unmounted" || echo "couldn't unmount $UKOL_PATH"
echo ""

echo "task 12: run 'mount -a' and check is $UKOL_PATH is mounted"
mount -a
mount
mount | grep -q $UKOL_PATH && echo "$UKOL_PATH is mounted" || echo "$UKOL_PATH wasn't mounted"
echo ""

echo "task 13: print info about all available packages from repo 'ukol'"
yum info --repo ukol --available
yum info --repo ukol --available | grep "$1" && echo "repo is available" || echo "repo is unavailable"
