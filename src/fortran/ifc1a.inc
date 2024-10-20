character(:), allocatable :: r
character(*), intent(in) :: path
logical, intent(in), optional :: strict, expand_tilde

character(kind=c_char, len=:), allocatable :: cbuf
integer(C_SIZE_T) :: N
logical(c_bool) :: s, e

allocate(character(max_path()) :: cbuf)
s = .false.
if(present(strict)) s = strict

e = .true.
if(present(expand_tilde)) e = expand_tilde

N = len(cbuf)
