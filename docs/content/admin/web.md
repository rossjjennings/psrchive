
# Update documentation

Documentation is published at [ReadTheDocs](https://psrchive.readthedocs.io/)
and managed in the `docs/` folder.
Each page is written using [Markdown](https://www.markdownguide.org/)
and the HTML is generated using [Sphinx](https://www.sphinx-doc.org/).

The API Reference documentation 
is generated using a combination of [Doxygen](https://www.doxygen.nl/), 
[Breathe](https://breathe.readthedocs.io), and Python scripts that are managed in 
the [Build-Commons](https://gitlab.com/pulsar-astronomy/build-commons) project.

The Build-Commons repository is included as a Git submodule in the `.build-commons` folder.
If this folder does not already exist,

```sh
git submodule update --init
```

To install the necessary Python packages,

```sh
pip install -r .build-commons/docs/requirements.txt
```

To generate the HTML,

```sh
cd docs/
make html
```

The top-level HTML file is output is output in `_build/html/index.html`.

