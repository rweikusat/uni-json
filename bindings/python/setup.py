from setuptools import setup, Extension

setup(
    name = "UniJson",
    author = "Rainer Weikusat",
    author_email = "rweikusat@talktalk.net",
    description = "Interface to uni-json JSON parser/ serialize",
    license = "MIT",
    ext_modules = [Extension('UniJson', ['parser.c'])]
)
