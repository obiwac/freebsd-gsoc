dnl #
dnl # Set the target cpu architecture.  This allows the
dnl # following syntax to be used in a Makefile.am.
dnl #
dnl # if TARGET_CPU_POWERPC
dnl # ...
dnl # else
dnl # ...
dnl # endif
dnl #
AC_DEFUN([ZFS_AC_CONFIG_ALWAYS_ARCH], [
	case $target_cpu in
	i?86)
		TARGET_CPU=i386
		;;
	amd64|x86_64)
		TARGET_CPU=x86_64
		;;
	powerpc*)
		TARGET_CPU=powerpc
		;;
	aarch64*)
		TARGET_CPU=aarch64
		;;
	armv*)
		TARGET_CPU=arm
		;;
	sparc64)
		TARGET_CPU=sparc64
		;;
	*)
		TARGET_CPU=$target_cpu
		;;
	esac

	AM_CONDITIONAL([TARGET_CPU_AARCH64], test $TARGET_CPU = aarch64)
	AM_CONDITIONAL([TARGET_CPU_X86_64],  test $TARGET_CPU = x86_64)
	AM_CONDITIONAL([TARGET_CPU_POWERPC], test $TARGET_CPU = powerpc)
	AM_CONDITIONAL([TARGET_CPU_SPARC64], test $TARGET_CPU = sparc64)
	AM_CONDITIONAL([TARGET_CPU_ARM],     test $TARGET_CPU = arm)
])
