# Copyright (c) 2026 Renesas Electronics Corporation
# SPDX-License-Identifier: Apache-2.0

board_runner_args(jlink "--device=R7FA2E2A7")
board_runner_args(pyocd "--target=R7FA2E2A7")
board_runner_args(rfp "--device=RA" "--tool=e2l" "--interface=swd" "--erase")

include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
include(${ZEPHYR_BASE}/boards/common/rfp.board.cmake)
