from setuptools import setup, Extension

setup(
    #*  meta
    #
    name = "UniJson",
    version = "0.1",
    description = "Interface to uni-json JSON parser/ serializer",
    license = "MIT",
    author = "Rainer Weikusat",
    author_email = "rweikusat@talktalk.net",

    #*  build info
    #
    ext_modules = [Extension('UniJson', ['src/parser.c'])]
)
