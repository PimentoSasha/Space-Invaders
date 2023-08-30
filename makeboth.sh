#! /bin/bash
pushd /code/riscv-firmware && make && popd
pushd /code/riscv-cart && make && popd