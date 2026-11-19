# Copyright The Zephyr Project Contributors
# SPDX-License-Identifier: Apache-2.0

board_runner_args(jlink "--device=GD32F303CCT6" "--speed=4000")
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)

board_runner_args(gd32isp "--device=GD32F303CCT6")
include(${ZEPHYR_BASE}/boards/common/gd32isp.board.cmake)
