#!/bin/sh

value=`%{_bindir}/systemctl is-active deviced`

if [ $value == "active" ]; then
	%{_bindir}/systemctl start mtp-initiator.service
fi