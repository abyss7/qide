#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

os.system("git -C `dirname $0` log --oneline master | wc -l")
