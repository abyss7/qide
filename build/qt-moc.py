#!/usr/bin/env python
# -*- coding: utf-8 -*-

import subprocess
import sys

subprocess.check_call(['moc-qt5'] + sys.argv[1:])
