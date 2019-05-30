# Portando o epos para o cortex A9

## Arquivos/links úteis

[TCC do cara que portou o epos pra a9](https://repositorio.ufsc.br/handle/123456789/184252)  
[Documentação da RealView A9](http://infocenter.arm.com/help/topic/com.arm.doc.dui0440b/index.html)

## TODO list

Em ordem de prioridade:

- [x] Fazer o escalonador funcionar.
- [ ] Verificar porque a LM3S811 dá erro de `out of memory`. Porque acho que vai acontecer também com a realview se não resolver nela primeiro.
- [ ] Mapear a memória da realview no respectivo arquivo `include/cortex_a/realview`. [Link referência](http://infocenter.arm.com/help/topic/com.arm.doc.dui0440b/index.html).
- [ ] Implementar o Timer_init
  - Seguir a base do cortex-m
- [ ] Interrupções:
  - [ ] Setar todas as interrupções para `int_not()`, usando um laço de 0-64 em IC::init().
  - [ ] Desabilitar as interrupções da CPU, chamando CLI (a.k.a. clear interruption flag).
  - [ ] (?) Implementar a Engine::init()
  - [ ] (?) Implementar a Engine::disable()
  - [ ] (?) Implementar a Engine::enable()
  - [ ] IC::int_vector
  - [ ] IC::enable()

## Algumas observações talvez importantes

- WFI (wait for interrupt) é o equivalente ao HALT do x86.
- `pc_setup.cc` linhas 420-427, aparentemente reserva espaço de memória para as entidades que serão utilizadas durante o setup.
- `src/init/init_system.cc` esse arquivo é importante... por algum motivo.