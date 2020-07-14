program main
implicit none
      integer, dimension(4, 4) :: A, B, C
      integer :: i, j
      A = 4
      do i = 1, 4
                do j = 1, 4
                        B(i, j) = i + j + 1
                        C(i, j) = A(i, j) + B(i, j)
                        if (C(i, j) == 8) then
                                C(i, j) = 16
                        end if
                end do
      end do        
     
      print*, "A: "      
      do i = 1, 4
                print*, A(i, :)
      end do
      print*, "B: "      
      do i = 1, 4
                print*, B(i, :)
      end do       
      print*, "C: "      
      do i = 1, 4
                print*, C(i, :)
      end do       
end program main 
