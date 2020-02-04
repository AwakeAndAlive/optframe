OptFrame
========

<p align="center">
  <a href="https://github.com/optframe/optframe">
    <img src="https://tokei.rs/b1/github/optframe/optframe?category=lines" alt="Current total lines.">
  </a>
  <a href="https://github.com/optframe/optframe/blob/master/LICENSE">
    <img src="https://img.shields.io/badge/License-LGPL%20v3-blue.svg" alt="License.">
  </a>
</p>

## OptFrame 4.0-dev - Optimization Framework

OptFrame is coming to its fourth generation! Now, with newest C++20 features and integrated Unit Testing for user projects (and examples). Scanner++ library is also dettached independent project now.
Remember to clone this repo using `--recursive` option, so you don't miss anything ;)

Check [https://optframe.github.io/docs/quick-start-guide/](https://optframe.github.io/docs/quick-start-guide/)

## Project Structure

Project is organized in three main folders: `src`, `tests` and `bin`.
* On `src/OptFrame` and `tests/OptFrame` are code and testing for OptFrame framework itself.
* On `src/Examples` and `tests/Examples` you can find code and tests for the provided Examples.
* On `src/MyProjects` and `tests/MyProjects` you can find code and tests for your own codes.
* On `bin`, you can find classic `mct.sh` script, that will generate a new project `X` on `src/MyProjects/X` folder (and tests on `tests/MyProjects/X`).

Tests are provided via `gtestlib` (on root folder), and you can also find `scannerpplib` (Scanner++ library) there.
To build `gtestlib`, just `cd gtestlib/build && cmake .. && make`.

## Concepts

A major change happened from OptFrame v3 to v4, where C++ Concepts were finally adopted (before official release of C++20 we use `concepts lite` on `gcc 7`).
This allowed a further simplification of project, by dividing most templates into two basic categories:
- Solution Space: `XS` template concept
- Objective Space: `XEv` template concept

Note that this will be valid for both Single- and Multi-Objective problems, unifying even more all kinds of techniques, into basic containers.
Basic containers are still default `Solution` and `Evaluation`. Note that both are now not mandatory (thanks to concepts). It's still a very good deal to use them, as they implement clone() pattern that allow primitive types like `int` and `double` to be directly used on `Evaluation`, also using basic std structures like `vector` directly on `Solution`.
If you don't like them, feel free to just replace them (`XS` and `XEv` can become virtually anything you want).

OptFrame 3 relied strongly on templates `R` and `ADS`, but now those only exist for basic Solution containers, and are not mandatory anymore (yet, it may still be good to follow this amazing optimization pattern).

Things are still moving, more big changes are coming.


## Submodules

Getting submodules: `git submodule update --init --recursive` and `git pull --recurse-submodules`.

## Using MCT script - Make a Compilable Thing!

Just type `cd ./bin && ./mct.sh` (or `make mct`) and follow the instructions on screen.
You can find your new project on `src/MyProjects/` (and tests on `tests/MyProjects/`).

## Special thanks for SourceForge.net

The OptFrame project was hosted in SourceForge.net for 8 years and we are
very grateful for all support during this time. Thanks to this support, we
have published several academic papers and solved many large scale problems
during these years. In order to easily interact with new project collaborators
the project is now moved to GitHub platform.

Thanks a lot SourceForge/GitHub!

## Citation

Cite this in your paper as:

```bibtex
@article{optframe2010,
    author = {Igor Coelho and Sabir Ribas and Mário Henrique de Paiva Perché and Pablo Munhoz and Marcone Souza and Luiz Ochi},
    year = {2010},
    month = {08},
    pages = {1887-1898},
    title = {OptFrame: a computational framework for combinatorial optimization problems},
    journal = "Simpósio Brasileiro de Pesquisa Operacional"
}
```
  
--

[**LICENSE GNU Lesser General Public License v3**](https://github.com/optframe/optframe/blob/master/LICENSE)

*OptFrame maintainers* [@igormcoelho](https://github.com/igormcoelho) and [@vncoelho](https://github.com/vncoelho)

Copyright (C) 2007-2019

The OptFrame team
