#include "raq.h"
#include <stdexcept>
#include <iostream>
#include <cmath>

RAQ::RAQ(std::vector<float> data){


  //implement double for loop to iterate through data
  for (unsigned int i = 0; i < data.size(); i++) {
		
	  //calculate starting average and add it to averages
	  float avg = data.at(i);
	  std::vector<float> newRow = { avg };
	  for (unsigned int j = i; j < data.size() - 1; j++) {

		  //update starting average for each next element
		  avg = ((avg * (j - i + 1)) + data.at(j + 1)) / (j - i + 2);
		  newRow.push_back(avg);
	  }
	  m_averages.push_back(newRow);
  }
}

float RAQ::query(int i, int j) const{
  //implement QUERY for index i to j
  //because the subvector indices start @0 not i, the index
  //containing the correct value is j - i
  if (i < 0 || j >= (int)m_averages.size())
	  throw std::domain_error("ERROR: Invalid bounds");
  if (i > j) {
    throw std::domain_error("ERROR: Bounds are backwards");
  }
  return m_averages[i][j-i];
}

void RAQ::dump() const{
  //implement dumping of averages vector
  for (unsigned int i = 0; i < m_averages.size(); i++) {
	  for (unsigned int j = 0; j < m_averages.at(i).size(); j++) {
		  std::cout << "Avg (" << i << ", " << j + i << "): " << m_averages[i][j] << std::endl;
	  }
  }
}

BlockRAQ::BlockRAQ(std::vector<float> data){
  //implement BLOCK PRECOMPUTATION
  //set blocksize equal to sqrt(size)
  m_blockSize = (int)sqrt((float)data.size());
  m_endBlockSize = data.size() % m_blockSize;
  m_dataSize = data.size();
  float sum = 0;
  unsigned int i = 0;
  m_data = data;
  //iterate through data and find the average.  
  while (i < data.size()) {
	  sum += data.at(i);
    //if the last block is the same size as the others 
    if ((i < (data.size() - m_endBlockSize)) && ((i + 1) % m_blockSize) == 0) {
      sum = (float) (sum / m_blockSize);
      m_blockAverages.push_back(sum);
      sum = 0;
	  }

    //if the last block is irregular
    else if (i == m_data.size() - 1) {
      sum = (float)(sum / m_endBlockSize);
      m_blockAverages.push_back(sum);
      sum = 0;
    }

    ++i;
  }
}

float BlockRAQ::query(int i, int j) const {
  if (i < 0 || j > (int)m_data.size() - 1)
    throw std::domain_error("Error: Invalid bounds");
  if (j < i)
    throw std::domain_error("Error: Bounds are backwards");
  //implement QUERY
  //find which blocks are in use (if any)
  //finding nearest start of a block to i and nearest end to j
  int nStart = i, nEnd = j; 
  float average = -1;

  while (nStart % m_blockSize != 0) {
    nStart++;
  }
  if (nStart > (int)m_data.size())
    nStart = m_blockSize * (m_blockAverages.size() - 1);

  while ((nEnd + 1) % m_blockSize != 0) {
    nEnd--;
  }

  //now, to find average you find the sum of the blocks bounded by
  //nStart and nEnd, and the sum of the elements bounded by i - nStart
  //and nEnd - j. 
  //IF nStart > nEnd, then no block is fully traversed so basic average is
  //computed 
  //IF j IS AT THE END OF THE LIST, REMEMBER THAT LAST BLOCK CAN BE SMALLER
  if (j != (int)m_data.size() - 1) {
    
    //at least one block is fully traversed
    if (nEnd > nStart) {
      int blockSum = 0, elemSum = 0;
      
      //calculate sum of blocks
      if (nStart < nEnd) {
        for (int n = (nStart / m_blockSize); n <= (nEnd + 1 - m_blockSize) / m_blockSize; n++) {
          blockSum += (m_blockAverages.at(n) * m_blockSize);
        }
      }
      if (nStart > i) {
        //calculate sum of elements from i to one before nStart
        for (int n = i; n < nStart; n++) {
          elemSum += m_data.at(n);
        }
      }
      if (nEnd < j) {
        //calculate sum of elements from one past nEnd to j
        for (int n = nEnd + 1; n <= j; n++) {
          elemSum += m_data.at(n);
        }
      }
      average = ((float)(elemSum + blockSum)) / ((float)(j - i + 1));
    }

    //less than one block is fully traversed
    else if (nEnd < nStart) {
      float sum = 0;
      
      for (int n = i; n <= j; n++) {
        sum += m_data.at(n);
      }

      average = sum / (float)(j - i + 1);
    }
  }
  else {
    //j IS the last element 
    //if the size is irregular, then nEnd < nStart means that either i is inside
    // the last block or in the block adjacent
    //if nEnd > nStart there are at least 2 blocks fully traversed (final and the 
    // one after i)
    if (nStart > nEnd) {
      //if i < nStart: if i is outside of the last block
      if (i < nStart) {
        float elemSum = 0;
        float blockSum = m_blockAverages.at(m_blockAverages.size() - 1) * m_endBlockSize;
        for (int n = i; n < nStart; n++) {
          elemSum += m_data.at(n);
        }
        average = (elemSum + blockSum) / (float)(j - i + 1);
      }
      //else, if i and j exactly span last block
      else if (i == nStart) {
        
        average = m_blockAverages.at(m_blockAverages.size() - 1);
      }
      //else, i is inside the last block
      else {
        float sum = 0;
        for (int n = i; n <= j; n++)
          sum += m_data.at(n);
        average = sum / (float)(j - i + 1);
      }
    }
    else if (nEnd > nStart) {
      float blockSum = 0, elemSum = 0;

      //calculate sum of blocks
      for (int n = (nStart / m_blockSize); n < (int)m_blockAverages.size(); n++) {
        if ((n < (int)m_blockAverages.size() - 1) || m_data.size() % m_blockSize == 0){
          blockSum += (m_blockAverages.at(n) * m_blockSize);
          
        }
        else if (m_data.size() % m_blockSize != 0)
          blockSum += (m_blockAverages.at(n) * m_endBlockSize);
      }

      //calculate sum of elements from i to one before nStart
      for (int n = i; n < nStart; n++) {
        elemSum += m_data.at(n);
      }

      average = ((float)(elemSum + blockSum)) / ((float)(j - i + 1));
    }
  }

  return average;
}

void BlockRAQ::dump() const{
  //implement dump of m_averages
  int startCell = 0;
  int endCell = m_blockSize - 1;
  for (unsigned int i = 0; i < m_blockAverages.size(); i++) {
    std::cout << "Avg (" << startCell << ", " << endCell << "): "
      << m_blockAverages.at(i) << std::endl;
    startCell += m_blockSize;
    endCell += m_blockSize;
    if (endCell > (int)m_data.size())
      endCell = m_data.size() - 1;

  }
}