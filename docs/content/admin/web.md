
# Update documentation

Documentation is published at [ReadTheDocs](https://psrchive.readthedocs.io/)
and managed in the `docs/` folder.  Each page is written using Markdown
and the HTML is generated using a combination of `doxygen`, `breathe-apidocs`,
and `sphinx`.

To install the necessary Python packages,
```
git submodule update --init
pip install -r .build-commons/docs/requirements.txt
```

To generate the HTML,

```
cd docs/
make html
```

The top-level HTML file is output is output in `_build/html/index.html`.

