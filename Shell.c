/ *   C o n s t r u c t i o n   d e s   a r b r e s   r e p r � s e n t a n t   d e s   c o m m a n d e s   * / 
 
 # i n c l u d e   " S h e l l . h " 
 # i n c l u d e   " A f f i c h a g e . h " 
 # i n c l u d e   " E v a l u a t i o n . h " 
 
 # i n c l u d e   < s t d b o o l . h > 
 # i n c l u d e   < s t d i o . h > 
 # i n c l u d e   < r e a d l i n e / r e a d l i n e . h > 
 # i n c l u d e   < r e a d l i n e / h i s t o r y . h > 
 # i n c l u d e   < s y s / w a i t . h > 
 
 e x t e r n   i n t   y y p a r s e _ s t r i n g ( c h a r   * ) ; 
 
   b o o l   i n t e r a c t i v e _ m o d e   =   1 ;   / /   p a r   d � f a u t   o n   u t i l i s e   r e a d l i n e   
   i n t   s t a t u s   =   0 ;   / /   v a l e u r   r e t o u r n � e   p a r   l a   d e r n i � r e   c o m m a n d e 
 
 
 / * 
   *   C o n s t r u i t   u n e   e x p r e s s i o n   �   p a r t i r   d e   s o u s - e x p r e s s i o n s 
   * / 
 E x p r e s s i o n   * C o n s t r u i r e N o e u d   ( e x p r _ t   t y p e ,   E x p r e s s i o n   * g ,   E x p r e s s i o n   * d ,   c h a r   * * a r g s ) 
 { 
     E x p r e s s i o n   * e ; 
           
     i f   ( ( e   =   ( E x p r e s s i o n   * ) m a l l o c ( s i z e o f ( E x p r e s s i o n ) ) )   = =   N U L L ) { 
         p e r r o r ( " m a l l o c " ) ; 
         e x i t ( E X I T _ F A I L U R E ) ; 
     } 
 
     e - > t y p e       =   t y p e ; 
     e - > g a u c h e   =   g ; 
     e - > d r o i t e   =   d ; 
     e - > a r g u m e n t s   =   a r g s ; 
     r e t u r n   e ; 
 }   / *   C o n s t r u i r e N o e u d   * / 
 
 
 
 / * 
   *   R e n v o i e   l a   l o n g u e u r   d ' u n e   l i s t e   d ' a r g u m e n t s 
   * / 
 i n t   L o n g u e u r L i s t e ( c h a r   * * l ) 
 { 
     c h a r   * * p ; 
     f o r   ( p = l ;   * p   ! =   N U L L ;   p + + ) 
         ; 
     r e t u r n   p - l ; 
 }   / *   L o n g u e u r L i s t e   * / 
 
 
 
 / * 
   *   R e n v o i e   u n e   l i s t e   d ' a r g u m e n t s ,   l a   p r e m i � r e   c a s e   � t a n t   i n i t i a l i s � e   �   N U L L ,   l a 
   *   l i s t e   p o u v a n t   c o n t e n i r   N B _ A R G S   a r g u m e n t s   ( p l u s   l e   p o i n t e u r   N U L L   d e   f i n   d e 
   *   l i s t e ) 
   * / 
 c h a r   * * I n i t i a l i s e r L i s t e A r g u m e n t s   ( v o i d ) 
 { 
     c h a r   * * l ; 
     
     l   =   ( c h a r   * * )   ( c a l l o c   ( N B _ A R G S + 1 ,   s i z e o f   ( c h a r   * ) ) ) ; 
     * l   =   N U L L ; 
     r e t u r n   l ; 
 }   / *   I n i t i a l i s e r L i s t e A r g u m e n t s   * / 
 
 
 
 / * 
   *   A j o u t e   e n   f i n   d e   l i s t e   l e   n o u v e l   a r g u m e n t   e t   r e n v o i e   l a   l i s t e   r � s u l t a n t e 
   * / 
 c h a r   * * A j o u t e r A r g   ( c h a r   * * L i s t e ,   c h a r   * A r g ) 
 { 
     c h a r   * * l ; 
     
     l   =   L i s t e   +   L o n g u e u r L i s t e   ( L i s t e ) ; 
     * l   =   ( c h a r   * )   ( m a l l o c   ( 1 + s t r l e n   ( A r g ) ) ) ; 
     s t r c p y   ( * l + + ,   A r g ) ; 
     * l   =   N U L L ; 
     r e t u r n   L i s t e ; 
 }   / *   A j o u t e r A r g   * / 
 
 
 
 / * 
   *   F o n c t i o n   a p p e l � e   l o r s q u e   l ' u t i l i s a t e u r   t a p e   "  " . 
   * / 
 v o i d   E n d O f F i l e   ( v o i d ) 
 { 
         p r i n t f ( " \ n " ) ; 
         e x i t   ( 0 ) ; 
 }   / *   E n d O f F i l e   * / 
 
 / * 
   *   A p p e l � e   p a r   y y p a r s e ( )   s u r   e r r e u r   s y n t a x i q u e 
   * / 
 v o i d   y y e r r o r   ( c h a r   * s ) 
 { 
       f p r i n t f ( s t d e r r ,   " % s \ n " ,   s ) ; 
 } 
 
 
 / * 
   *   L i b � r a t i o n   d e   l a   m � m o i r e   o c c u p � e   p a r   u n e   e x p r e s s i o n 
   * / 
 v o i d 
 e x p r e s s i o n _ f r e e ( E x p r e s s i o n   * e ) 
 { 
     i f   ( e   = =   N U L L ) 
         r e t u r n ; 
         
     e x p r e s s i o n _ f r e e ( e - > g a u c h e ) ; 
     e x p r e s s i o n _ f r e e ( e - > d r o i t e ) ; 
 
     i f   ( e - > a r g u m e n t s   ! =   N U L L ) 
         { 
             f o r   ( i n t   i   =   0 ;   e - > a r g u m e n t s [ i ]   ! =   N U L L ;   i + + ) 
 	 f r e e ( e - > a r g u m e n t s [ i ] ) ; 
             f r e e ( e - > a r g u m e n t s ) ;     
         } 
 
     f r e e ( e ) ; 
 } 
 
 i n t   p r i n t P w d ( )   
 { 
     c h a r   p w d [ 5 1 2 ] ;                                       
     g e t c w d ( p w d ,   s i z e o f ( p w d ) ) ; 
       p r i n t f   ( " \ x 1 b [ 3 3 m % s   \ x 1 b [ 0 m " ,   p w d ) ; 
     r e t u r n   0 ; 
 } 
     
 
 
 / * 
   *   L e c t u r e   d e   l a   l i g n e   d e   c o m m a n d e   �   l ' a i d e   d e   r e a d l i n e   e n   m o d e   i n t e r a c t i f 
   *   M � m o r i s a t i o n   d a n s   l ' h i s t o r i q u e   d e s   c o m m a n d e s 
   *   A n a l y s e   d e   l a   l i g n e   l u e   
   * / 
 
 i n t 
 m y _ y y p a r s e ( v o i d ) 
 { 
     i f   ( i n t e r a c t i v e _ m o d e ) 
         { 
             c h a r   * l i n e   =   N U L L ; 
             c h a r   b u f f e r [ 1 0 2 4 ] ; 
             p r i n t P w d ( ) ; 
             s n p r i n t f ( b u f f e r ,   1 0 2 4 ,   " ( % d ) : " ,   s t a t u s ) ; 
             l i n e   =   r e a d l i n e ( b u f f e r ) ; 
             i f ( l i n e   ! =   N U L L ) 
 	 { 
 	     i n t   r e t ; 
 	     a d d _ h i s t o r y ( l i n e ) ;                             / /   E n r e g i s t r e   l a   l i n e   n o n   v i d e   d a n s   l ' h i s t o r i q u e   c o u r a n t 
 	     * s t r c h r ( l i n e ,   ' \ 0 ' )   =   ' \ n ' ;             / /   A j o u t e   \ n   �   l a   l i n e   p o u r   q u ' e l l e   p u i s s e   e t r e   t r a i t �   p a r   l e   p a r s e u r 
 	     r e t   =   y y p a r s e _ s t r i n g ( l i n e ) ;           / /   R e m p l a c e   l ' e n t r � e   s t a n d a r d   d e   y y p a r s e   p a r   s 
 	     f r e e ( l i n e ) ; 
 	     r e t u r n   r e t ; 
 	 } 
             e l s e 
 	 { 
 	     E n d O f F i l e ( ) ; 
 	     r e t u r n   - 1 ; 
 	 } 
         } 
     e l s e 
         { 
             / /   p o u r   l e   m o d e   d i s t a n t   p a r   e x e m p l e 
             
             i n t   r e t ;   i n t   c ; 
                     
             c h a r   * l i n e   =   N U L L ; 
             s i z e _ t   l i n e c a p   =   0 ; 
             s s i z e _ t   l i n e l e n ; 
             l i n e l e n   =   g e t l i n e ( & l i n e ,   & l i n e c a p ,   s t d i n ) ; 
 
             i f ( l i n e l e n > 0 ) 
 	 { 
 	     i n t   r e t ; 
 	     r e t   =   y y p a r s e _ s t r i n g ( l i n e ) ;     
 	     f r e e ( l i n e ) ; 
 	     r e t u r n   r e t ; 
 	 }         
         } 
 } 
 
 
 
             / * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - . 
             |   L o r s q u e   l ' a n a l y s e   d e   l a   l i g n e   d e   c o m m a n d e   e s t   e f f e c t u � e   s a n s   e r r e u r .   L a   v a r i a b l e             | 
             |   g l o b a l e   E x p r e s s i o n A n a l y s e e   p o i n t e   s u r   u n   a r b r e   r e p r � s e n t a n t   l ' e x p r e s s i o n .     L e   t y p e         | 
             |               " E x p r e s s i o n "   d e   l ' a r b r e   e s t   d � c r i t   d a n s   l e   f i c h i e r   S h e l l . h .   I l   c o n t i e n t   4               | 
             |               c h a m p s .   S i   e   e s t   d u   t y p e   E x p r e s s i o n   : 	 	 	 	 	             | 
             |   	 	 	 	 	 	 	 	 	 	             | 
             |   -   e . t y p e   e s t   u n   t y p e   d ' e x p r e s s i o n ,   c o n t e n a n t   u n e   v a l e u r   d � f i n i e   p a r   � n u m � r a t i o n   d a n s     | 
             |       S h e l l . h .   C e t t e   v a l e u r   p e u t   � t r e   : 	 	 	 	 	             	             | 
             |   	 	 	 	 	 	 	 	 	 	             | 
             |       -   V I D E ,   c o m m a n d e   v i d e 	 	 	 	 	 	 	 	             | 
             |       -   S I M P L E ,   c o m m a n d e   s i m p l e   e t   s e s   a r g u m e n t s 	 	 	 	 	             | 
             |       -   S E Q U E N C E ,   s � q u e n c e   ( ; )   d ' i n s t r u c t i o n s 	 	 	 	 	             | 
             |       -   S E Q U E N C E _ E T ,   s � q u e n c e   c o n d i t i o n n e l l e   ( & & )   d ' i n s t r u c t i o n s 	 	 	             | 
             |       -   S E Q U E N C E _ O U ,   s � q u e n c e   c o n d i t i o n n e l l e   ( | | )   d ' i n s t r u c t i o n s 	 	 	             | 
             |       -   B G ,   t � c h e   e n   a r r i � r e   p l a n   ( & ) 	 	 	 	 	 	             | 
             |       -   P I P E ,   p i p e   ( | ) . 	 	 	 	 	 	 	 	             | 
             |       -   R E D I R E C T I O N _ I ,   r e d i r e c t i o n   d e   l ' e n t r � e   ( < ) 	 	 	 	 	             | 
             |       -   R E D I R E C T I O N _ O ,   r e d i r e c t i o n   d e   l a   s o r t i e   ( > ) 	 	 	 	 	             | 
             |       -   R E D I R E C T I O N _ A ,   r e d i r e c t i o n   d e   l a   s o r t i e   e n   m o d e   A P P E N D   ( > > ) . 	 	             | 
             |       -   R E D I R E C T I O N _ E ,   r e d i r e c t i o n   d e   l a   s o r t i e   e r r e u r ,     	       	 	 	             | 
             |       -   R E D I R E C T I O N _ E O ,   r e d i r e c t i o n   d e s   s o r t i e s   e r r e u r   e t   s t a n d a r d . 	 	 	             | 
             |   	 	 	 	 	 	 	 	 	 	             | 
             |   -   e . g a u c h e   e t   e . d r o i t e ,   d e   t y p e   E x p r e s s i o n   * ,   r e p r � s e n t e n t   u n e   s o u s - e x p r e s s i o n   g a u c h e   | 
             |               e t   u n e   s o u s - e x p r e s s i o n   d r o i t e .   C e s   d e u x   c h a m p s   n e   s o n t   p a s   u t i l i s � s   p o u r   l e s         | 
             |               t y p e s   V I D E   e t   S I M P L E .   P o u r   l e s   e x p r e s s i o n s   r � c l a m a n t   d e u x   s o u s - e x p r e s s i o n s             | 
             |               ( S E Q U E N C E ,   S E Q U E N C E _ E T ,   S E Q U E N C E _ O U ,   e t   P I P E )   c e s   d e u x   c h a m p s   s o n t   u t i l i s � s           | 
             |               s i m u l t a n n � m e n t .     P o u r   l e s   a u t r e s   c h a m p s ,   s e u l e   l ' e x p r e s s i o n   g a u c h e   e s t 	             | 
             |               u t i l i s � e . 	 	 	 	 	 	 	 	 	             | 
             |   	 	 	 	 	 	 	 	 	 	             | 
             |   -   e . a r g u m e n t s ,   d e   t y p e   c h a r   * * ,   a   d e u x   i n t e r p r e t a t i o n s   : 	 	 	             | 
             |   	 	 	 	 	 	 	 	 	 	             | 
             |             -   s i   l e   t y p e   d e   l a   c o m m a n d e   e s t   s i m p l e ,   e . a r g u m e n t s   p o i n t e   s u r   u n   t a b l e a u   �   l a       | 
             |               a r g v .   ( e . a r g u m e n t s ) [ 0 ]   e s t   l e   n o m   d e   l a   c o m m a n d e ,   ( e . a r g u m e n t s ) [ 1 ]   e s t   l e 	             | 
             |               p r e m i e r   a r g u m e n t ,   e t c . 	 	 	 	 	 	 	             | 
             |   	 	 	 	 	 	 	 	 	 	             | 
             |             -   s i   l e   t y p e   d e   l a   c o m m a n d e   e s t   u n e   r e d i r e c t i o n ,   ( e . a r g u m e n t s ) [ 0 ]   e s t   l e   n o m   d u     | 
             |               f i c h i e r   v e r s   l e q u e l   o n   r e d i r i g e . 	 	 	 	 	 	             | 
             ` - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - * / 
 
 
 i n t 
 m a i n   ( i n t   a r g c ,   c h a r   * * a r g v ) 
 { 
     / /   f a i r e   e n   s o r t e   q u ' i n t e r a c t i v e _ m o d e   =   0   l o r s q u e   l e   s h e l l   e s t   d i s t a n t   
     i f   ( i n t e r a c t i v e _ m o d e ) 
         { 
             u s i n g _ h i s t o r y ( ) ; 
         } 
     e l s e 
         { 
             / /     m o d e   d i s t a n t   
         } 
     
     w h i l e   ( 1 ) { 
         i f   ( m y _ y y p a r s e   ( )   = =   0 )   {     / *   L ' a n a l y s e   a   a b o u t i   * / 
             a f f i c h e r _ e x p r ( E x p r e s s i o n A n a l y s e e ) ; 
             f f l u s h ( s t d o u t ) ; 
             s t a t u s   =   e v a l u e r _ e x p r ( E x p r e s s i o n A n a l y s e e ) ; 
             e x p r e s s i o n _ f r e e ( E x p r e s s i o n A n a l y s e e ) ; 
 
             i n t   p i d , s t a t u s ; 
             w h i l e (   ( p i d   =   w a i t p i d ( - 1 , & s t a t u s , W N O H A N G ) )   >   0 ) 
 	     p r i n t f ( " l e   p r o c e s s u s   % d   e s t   f i n i   ( % d ) \ n " , 
 	 	   p i d , 
 	 	   W I F E X I T E D ( s t a t u s )   ?   W E X I T S T A T U S ( s t a t u s )   :   1 2 8 + W T E R M S I G ( s t a t u s ) ) ; 
         } 
         e l s e   { 
             / *   L ' a n a l y s e   d e   l a   l i g n e   d e   c o m m a n d e   a   d o n n �   u n e   e r r e u r   * / 
         } 
     } 
     r e t u r n   0 ; 
 } 
 