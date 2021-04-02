Esta carpeta contiene una definición de una clase para
el cómputo de descriptores de Fourier de una silueta.

La clase esta definida en los siguientes archivos:
  * FourierDescriptor.cpp
  * include/FourierDescriptor.h

Ejemplos de uso de la clase son los siguientes:
   * exampleFourierDescriptors.cpp
      Carga una imagen binario, calcula el contorno y de este
      calcula "nDesc" los descriptores de Fourier, donde nDesc
      es el numero descriptores utilizados en la representación.
      El programa muestra en la pantalla la imagen original, su silueta
      extraida y la silueta reconstruida. En la consola se imprime el
      error de reconstrucción (error cuadrático medio).

   * exampleHuMomentsFourierDescriptors.cpp
      Hace lo mismo que el exampleFourierDescriptors.cpp pero ademas para
      cada silueta extraida calcula los momentos invariantes de Hu. 
      Jorge, vale la pena revisar este tutorial: 
      
      https://learnopencv.com/shape-matching-using-hu-moments-c-python/
