#!/usr/bin/env sh

git add .
git commit -sam "update @ $(date)"
git push -u origin "$(git rev-parse --abbrev-ref HEAD)"
